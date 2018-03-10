

1) git clone  git://anongit.freedesktop.org/gstreamer/gstreamer-vaapi
2) git checkout 1.8.3
3) vim ./gst-libs/gst/vaapi/Makefile.am 
	Add gstmyelement.c  to libgstvaapi_source_c
	Add gstmyelement.h to libgstvaapi_source_h
4) copy gstmyelement.{h,c} to gst-libs/gst/vaapi
5) in gst/vaapi/ create a file gstmyelem.h
	ifndef _GST_MYELEM_H_
	#define _GST_MYELEM_H_

	#include <gst/vaapi/gstmyelement.h>

	G_BEGIN_DECLS


	G_END_DECLS

	#endif /* GST_MYELEM_H_ */


6) in plugin_init() (in gst/vaapi/gstvaapi.c) add

#include "gstmyelem.h"

gst_element_register (plugin, "myelement",
      GST_RANK_PRIMARY, GST_TYPE_MYELEMENT);

7) ./autogen.sh
   sudo make (You will see libgstvaapi_la-gstmyelement.lo created)
   sudo make install	 	
8) cd /usr/local/lib/gstreamer-1.0
   sudo cp libgstvaapi.so /usr/lib/x86_64-linux-gnu/gstreamer-1.0

9) gst-inspect-1.0 myelement showuld give you 

Factory Details:
  Rank                     primary (256)
  Long-name                myelement
  Klass                    Passthrough Element
  Description              A Passthrough VA-API Element
  Author                   Mathew P Joseph <mathew.p.joseph@gmail.com>

Plugin Details:
  Name                     vaapi
  Description              VA-API based elements
  Filename                 /usr/lib/x86_64-linux-gnu/gstreamer-1.0/libgstvaapi.so
	.
	.
	.


