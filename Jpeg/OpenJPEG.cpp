#include <stdio.h>  // needed by jpeglib.h
#include <string.h>
#include <setjmp.h>
extern "C" {  // stupid JPEG library
  #include "jpeg-6b/jpeglib.h"
}
#include "Corona.h"
#include "SimpleImage.h"


namespace corona {

	static const int JPEG_BUFFER_SIZE = 4096;


	void    JPEG_init_source(j_decompress_ptr cinfo);
	boolean JPEG_fill_input_buffer(j_decompress_ptr cinfo);
	void    JPEG_skip_input_data(j_decompress_ptr cinfo, long num_bytes);
	void    JPEG_term_source(j_decompress_ptr cinfo);
	void    JPEG_error_exit(j_common_ptr cinfo);
	void    JPEG_emit_message(j_common_ptr cinfo, int msg_level);


	struct InternalStruct {
		struct {
			jpeg_error_mgr mgr;
			jmp_buf setjmp_buffer;
		} error_mgr;

		File* file;
		byte buffer[JPEG_BUFFER_SIZE];
	};

	//////////////////////////////////////////////////////////////////////////////

	Image* OpenJPEG(File* file, WriteJpegData callbackFunc, void* pUserData )
	{
		// set up internal information
		InternalStruct is;
		is.file = file;

		// initialize the source manager
		jpeg_source_mgr mgr;
		mgr.bytes_in_buffer = 0;
		mgr.next_input_byte = NULL;
		mgr.init_source       = JPEG_init_source;
		mgr.fill_input_buffer = JPEG_fill_input_buffer;
		mgr.skip_input_data   = JPEG_skip_input_data;
		mgr.resync_to_restart = jpeg_resync_to_restart;  // use default
		mgr.term_source       = JPEG_term_source;

		// initialize decompressor
		jpeg_decompress_struct cinfo;
		jpeg_create_decompress(&cinfo);
		cinfo.client_data = &is;

		cinfo.err = jpeg_std_error(&is.error_mgr.mgr);
		is.error_mgr.mgr.error_exit = JPEG_error_exit;

		SimpleImage* image = 0;

		if (setjmp(is.error_mgr.setjmp_buffer))
		{
			delete image;
			jpeg_destroy_decompress(&cinfo);
			return 0;
		}

		cinfo.src = &mgr;
		jpeg_read_header(&cinfo, TRUE);
		jpeg_start_decompress(&cinfo);

		// do we support the number of color components?
		if (cinfo.output_components != 1 && cinfo.output_components != 3)
		{
			jpeg_finish_decompress(&cinfo);
			jpeg_destroy_decompress(&cinfo);
			return 0;
		}

		// make a one-row-high sample array that will go away when done with image
		int row_stride = cinfo.output_width * cinfo.output_components;
		JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)(
			(j_common_ptr)&cinfo,
			JPOOL_IMAGE,
			row_stride,
			1);

		// If there is a pixel callback handler
		bool completedDecompression = true;
		if( callbackFunc )
		{
			while (cinfo.output_scanline < cinfo.output_height)
			{
				int numRows = jpeg_read_scanlines(&cinfo, buffer, 1);
				if (numRows == 0)
				{
					completedDecompression = false;
					break;
				}

				// Let the callback function handle the pixels
				callbackFunc( (uint8*)*buffer, numRows, pUserData );
			}
		}
		else
		{
			// allocate image
			unsigned width  = cinfo.output_width;
			unsigned height = cinfo.output_height;
			byte* pixels = new byte[width * height * 3];
			memset(pixels, 0, width * height * 3);

			// create the image object now, so that if the error handler is called,
			// the longjmp code will know what to free
			image = new SimpleImage(width, height, PF_R8G8B8, pixels);

			// read the scanlines
			while (cinfo.output_scanline < height)
			{
				int num_rows = jpeg_read_scanlines(&cinfo, buffer, 1);
				if (num_rows == 0)
				{
					completedDecompression = false;
					break;
				}

				// Copy scanline into pixel buffer, grayscale
				if (cinfo.output_components == 1)
				{ 
					byte* in = (byte*)(*buffer);
					for (unsigned i = 0; i < width * num_rows; ++i)
					{
						*pixels++ = *in; // red
						*pixels++ = *in; // green
						*pixels++ = *in; // blue
						++in;
					}
				}
				// RGB
				else if (cinfo.output_components == 3)
				{
					memcpy(pixels, (*buffer), num_rows * width * 3);
					pixels += num_rows * width * 3;
				}
			}
		}

		// finish up
		if (completedDecompression)
			jpeg_finish_decompress(&cinfo);

		jpeg_destroy_decompress(&cinfo);

		return image;
	}

	class JPEGReaderImpl : public JPEGReader
	{
	private:
		JSAMPARRAY m_Buffer;
		jpeg_decompress_struct m_CInfo;
		jpeg_source_mgr mgr;
		InternalStruct internalStruct;

	public:
		~JPEGReaderImpl()
		{
			Term();
		}

		bool Init( File* pFile )
		{
			// set up internal information			
			internalStruct.file = pFile;

			// initialize the source manager
			mgr.bytes_in_buffer = 0;
			mgr.next_input_byte = NULL;
			mgr.init_source       = JPEG_init_source;
			mgr.fill_input_buffer = JPEG_fill_input_buffer;
			mgr.skip_input_data   = JPEG_skip_input_data;
			mgr.resync_to_restart = jpeg_resync_to_restart;  // use default
			mgr.term_source       = JPEG_term_source;

			// initialize decompressor			
			jpeg_create_decompress(&m_CInfo);
			m_CInfo.client_data = &internalStruct;

			m_CInfo.err = jpeg_std_error(&internalStruct.error_mgr.mgr);
			internalStruct.error_mgr.mgr.error_exit = JPEG_error_exit;

			m_CInfo.src = &mgr;
			jpeg_read_header(&m_CInfo, TRUE);
			jpeg_start_decompress(&m_CInfo);

			// do we support the number of color components?
			if (m_CInfo.output_components != 1 && m_CInfo.output_components != 3)
			{
				jpeg_finish_decompress(&m_CInfo);
				jpeg_destroy_decompress(&m_CInfo);
				return false;
			}

			// make a one-row-high sample array that will go away when done with image
			int row_stride = m_CInfo.output_width * m_CInfo.output_components;
			m_Buffer = (*m_CInfo.mem->alloc_sarray)( (j_common_ptr)&m_CInfo,
													JPOOL_IMAGE,
													row_stride,
													1);

			return true;
		}

		virtual uint8* ReadLine()
		{
			int numRows = jpeg_read_scanlines(&m_CInfo, m_Buffer, 1);
			if (numRows == 0)
				return 0;

			return (uint8*)*m_Buffer;
		}

		void Term()
		{
			jpeg_finish_decompress(&m_CInfo);
			jpeg_destroy_decompress(&m_CInfo);
		}
	};

	JPEGReader* OpenJPEGReader( File* pFile )
	{
		JPEGReaderImpl* pNewReader = new JPEGReaderImpl();
		pNewReader->Init( pFile );

		return pNewReader;
	}

	//////////////////////////////////////////////////////////////////////////////

	void JPEG_init_source(j_decompress_ptr cinfo) {
		// no initialization required
	}

	//////////////////////////////////////////////////////////////////////////////

	boolean JPEG_fill_input_buffer(j_decompress_ptr cinfo) {
		// more or less copied from jdatasrc.c

		InternalStruct* is = (InternalStruct*)(cinfo->client_data);

		int nbytes = is->file->read(is->buffer, JPEG_BUFFER_SIZE);
		if (nbytes <= 0) {
			/* Insert a fake EOI marker */
			is->buffer[0] = (JOCTET)0xFF;
			is->buffer[1] = (JOCTET)JPEG_EOI;
			nbytes = 2;
		}

		cinfo->src->bytes_in_buffer = nbytes;
		cinfo->src->next_input_byte = is->buffer;
		return TRUE;
	}

	//////////////////////////////////////////////////////////////////////////////

	void JPEG_skip_input_data(j_decompress_ptr cinfo, long num_bytes) {
		//InternalStruct* is = (InternalStruct*)(cinfo->client_data);
		if (num_bytes > 0) {
			while (num_bytes > (long)cinfo->src->bytes_in_buffer) {
				num_bytes -= (long)cinfo->src->bytes_in_buffer;
				JPEG_fill_input_buffer(cinfo);
			}
			cinfo->src->next_input_byte += (size_t)num_bytes;
			cinfo->src->bytes_in_buffer -= (size_t)num_bytes;
		}
	}

	//////////////////////////////////////////////////////////////////////////////

	void JPEG_term_source(j_decompress_ptr cinfo) {
		// nothing to do here...
	}

	//////////////////////////////////////////////////////////////////////////////

	void JPEG_error_exit(j_common_ptr cinfo) {
		InternalStruct* is = (InternalStruct*)(cinfo->client_data);
		longjmp(is->error_mgr.setjmp_buffer, 1);
	}

	//////////////////////////////////////////////////////////////////////////////

	void JPEG_emit_message(j_common_ptr /*cinfo*/, int /*msg_level*/) {
		// ignore error messages
	}

  //////////////////////////////////////////////////////////////////////////////

}
