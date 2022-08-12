from time import sleep
import datetime
import sys, os
import requests
import firebase_admin
from firebase_admin import credentials
from firebase_admin import storage
from uuid import uuid4
import schedule

#my project id
PROJECT_ID = "cg24-8875b"

 
cred = credentials.Certificate("cg24-8875b-firebase-adminsdk-89mhn-55b53b5a18.json") #키 이름
default_app = firebase_admin.initialize_app(cred,{'storageBucket':f"cg24-8875b.appspot.com"})
#버킷은 바이너리 객체의 상위 컨테이너이다. 버킷은 Storage에서 데이터를 보관하는 기본 컨테이너이다.
bucket = storage.bucket()#기본 버킷 사용
 
def fileUpload(file):
    blob = bucket.blob('image_store/'+file) #저장한 사진을 파이어베이스 storage의 image_store라는 이름의 디렉토리에 저장
    #new token and metadata 설정
    new_token = uuid4()
    metadata = {"firebaseStorageDownloadTokens": new_token} #access token이 필요하다.
    blob.metadata = metadata
 
    #upload file
    blob.upload_from_filename(filename='/home/cg24/image_store/'+file, content_type='image/jpg') #파일이 저장된 주소와 이미지 형식
    #debugging hello
    print("hello ")
    print(blob.public_url)
    
def execute_camera():
    #사진찍기
    basename = "smr"
    filename = 'image.jpg'
 
    os.system('fswebcam -r 1280*960 -S 3 --jpeg 50 --save /home/cg24/image_store/image.jpg')
    sleep(5)
    fileUpload(filename)

# 메모리 카드의 파일을 정리 해 주자.
def clearAll():
    #제대로 할려면 용량 체크 하고 먼저 촬영된 이미지 부터 지워야 할것 같지만 여기선 폴더안에 파일을 몽땅 지우자.
    path = '/home/cg24/image_store'
    os.system('rm -rf %s/*' % path)
 
 
#10초 마다 실행
schedule.every(10).seconds.do(execute_camera)
#10분에 한번씩 실행
#schedule.every(10).minutes.do(execute_camera)
#매 시간 마다 실행
schedule.every().hour.do(clearAll)
#기타 정해진 시간에 실행/매주 월요일에 실행/매주 수요일 몇시에 실행 등의 옵션이 있다.
 
    
while True:
    schedule.run_pending()
    sleep(1)
