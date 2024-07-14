import argparse
import shutil
from ftp_helper import *

if __name__ == '__main__':
    print("Download test files from ftp")
    parser = argparse.ArgumentParser()
    parser.add_argument('--module', help='Set Module to test')

    arg_dict = parser.parse_args()

    test_script_zip = "test_script.tar.gz"
    image_zip = "{}.tar.gz".format(arg_dict.module)
    #connect ftp
    hsot, port, key = rs_ftp_host()
    sftp = SFTP(hsot, port, key)
    sftp.connect()
    dest_download_path = os.path.join(os.getcwd(), "SRC")

    sftp.download(os.path.join(rs_ftp_server(), test_script_zip), dest_download_path)
    sftp.download(os.path.join(rs_ftp_server(), image_zip), dest_download_path)
    sftp.disconnect()
    shutil.unpack_archive(os.path.join(dest_download_path, test_script_zip), os.path.join(os.getcwd(), "script"), format="gztar")
    image_dest_unpack_path = os.path.join(dest_download_path, arg_dict.module)
    os.remove(os.path.join(dest_download_path, test_script_zip))
    try:
        shutil.unpack_archive(os.path.join(dest_download_path, image_zip), image_dest_unpack_path, format="gztar")
        os.remove(os.path.join(dest_download_path, image_zip))
    except Exception as e:
        print("Unzip image zip failed: {}, mabye there is no {} exists or build failed".format(e, image_zip))
