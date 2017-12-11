/****************************************************
* 
* Program to use a sample plugin
*
* Pipeline : filesrc location <path> ! qtdemux ! 
*     h264parse ! gstplugin ! vaapidecode ! vaapisink
*
* gcc gst-plugin.cpp -o plugin \
*     `pkg-config --cflags --libs gstreamer-1.0
*
* Aint sure of the license... You are free to reach 
* me: Mathew P Joseph @ mathew.p.joseph@gmail.com
*
*****************************************************/


# include <gst/gst.h>
# include <glib.h>

static void on_pad_added( GstElement*, GstPad*, gpointer);


int main(int argc, char* argv[])
{

	GstElement *pipeline, *source, *demux, *parser, *myplugin, *decoder, *sink;
	GstBus *bus;
	GMainLoop *loop;

	/* Step 1: Initialization */
	
   	gst_init(NULL, NULL);

	loop = g_main_loop_new(NULL, FALSE);

	if(argc != 2)
		g_printerr("Usage: %s <Path to MP4 File> \n", argv[0]);

	/* Step 2: Create the gstreamer elements */

	pipeline = gst_pipeline_new("video-player");	    		
	source	 = gst_element_factory_make("filesrc", "file-source");
	demux	 = gst_element_factory_make("qtdemux", "demuxer"); 
	parser	 = gst_element_factory_make("h264parse", "h264parse");
	myplugin = gst_element_factory_make("plugin", "my-plugin");	 
	decoder	 = gst_element_factory_make("vaapidecode", "vaapi-decoder");
	sink	 = gst_element_factory_make("vaapisink", "vaapi-sink");
	
	//if(!pipeline || !source || !demux || !parser || !myplugin || !decoder || !sink){
	if(!pipeline || !source || !demux || !parser || !myplugin || !sink){
		g_printerr("One of the elements could not be created. Exiting ...\n");
		return -1;
	}

	/* Step 3: Set up the pipeline */
	
	/* Assign input filename to the source element */
	/* gst-inspect-1.0 filesrc
	* 	.
	*	.
	* 	Element Properties: 
	*	.
	*	.
	*	location            : Location of the file to read
        *             		      flags: readable, writable, changeable only in NULL or READY state
        *                	      String. Default: nul		
	*
	*/

	g_object_set (G_OBJECT (source), "location", argv[1], NULL);

	/* Step 4: Adding elements to the pipeline */
	gst_bin_add_many(GST_BIN(pipeline), source, demux, parser, myplugin, decoder, sink, NULL);	

	/* Step 5: Link the elements */
	/* The demux to parser cannot be linked immediately. 
	 * That needs to be handled in the callback		
	*/

	gst_element_link(source, demux);
	gst_element_link_many(parser, myplugin, decoder, sink, NULL);

	/* Step 6: Register a call back for pad-added signal */
	/* parser variable is being passed to the call back */
	g_signal_connect(demux, "pad-added", G_CALLBACK(on_pad_added), parser);

	/* Step 7: Set the pipeline to "playing" state */ 
	g_print(" Now playing %s \n", argv[1]);	
	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	/* Step 8: Start the loop */
	g_print(" Running ... \n");
	g_main_loop_run(loop);

	/* Step 9: Out of the main loop, clean up */
	gst_element_set_state(pipeline, GST_STATE_NULL);

	g_print("Deleting pipeline \n");
	gst_object_unref(GST_OBJECT(pipeline));
	g_main_loop_unref(loop);

	return 0;
}
	
static void on_pad_added( GstElement *element,
                          GstPad     *pad,
                          gpointer   data)
{
	GstPad *sinkpad;
	GstElement *parser = (GstElement *)data;

	g_print("Dynamic pad created, linking demuxer/parser \n");

	/* Get "sink" pad of the parser */
	sinkpad = gst_element_get_static_pad(parser, "sink");
	
	/* Link demux source pad to parser sink pad */
	gst_pad_link(pad, sinkpad);

	gst_object_unref(sinkpad);

}
	 


