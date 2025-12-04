
pip install -r requirements.txt 

python -m video_converter -i test_video/birds.mp4 -o birds.mjpeg -f mjpeg -q 85 
python -m video_converter -i test_video/birds.mp4 -o test_video/birds.avi    -f avi_mjpeg -r 25 
python -m video_converter -i test_video/birds.mp4 -o test_video/birds.h264  -f h264

python test_conversion.py  