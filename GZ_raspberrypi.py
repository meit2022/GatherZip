from time import sleep
import datetime
import sys, os
import requests
import firebase_admin
from firebase_admin import credentials
from firebase_admin import storage
from firebase_admin import db
from uuid import uuid4


PROJECT_ID = "cg24-8875b"
#my project id
 
cred = credentials.Certificate("cg24-8875b-firebase-adminsdk-89mhn-55b53b5a18.json") #(키 이름 ) 부분에 본인의 키이름을 적어주세요.
db_url= 'https://cg24-8875b-default-rtdb.firebaseio.com/'
default_app = firebase_admin.initialize_app(cred, {'databaseURL':db_url,'storageBucket':f"cg24-8875b.appspot.com" })
bucket = storage.bucket()


def fileUpload(file):
    blob = bucket.blob('image_store/'+file) #저장한 사진을 파이어베이스 storage의 image_store라는 이름의 디렉토리에 저장
    #new token and metadata 설정
    new_token = uuid4()
    metadata = {"firebaseStorageDownloadTokens": new_token} #access token이 필요하다.
    blob.metadata = metadata
 
    #upload file
    blob.upload_from_filename(filename='/home/cg24/image_store/'+file, content_type='image.jpg') #파일이 저장된 주소와 이미지 형식
    #debugging hello
    print("hello ")
    img = db.reference('image')
    img.update({'image':"%s" %(blob.public_url)} )
    print(blob.public_url)

    
def execute_camera():
    print("cabera")

    #사진찍기
    filename = 'image.jpg'
    os.system('fswebcam -r 960*960 -S 3 --jpeg 50 --save /home/cg24/image_store/image.jpg')
    sleep(5)

    #사진 파일을 파이어베이스에 업로드 한다.
    fileUpload(filename)
    #로컬 하드의 사진을 삭제한다.
    clearAll()

    

# 메모리 카드의 파일을 정리 해 주자.
def clearAll():
    #제대로 할려면 용량 체크 하고 먼저 촬영된 이미지 부터 지워야 할것 같지만 여기선 폴더안에 파일을 몽땅 지우자.
    path = '/home/cg24/image_store/image.jpg'
    os.remove('/home/cg24/image_store/image.jpg')
    print("done")
    

while 1:
    
    
    print("sending barcode")
    
    scode = str(input())
    
    print(scode)
    
    data = scode
    
    barcode = db.reference("CG24/IDs")
    barcode.update({'present_barcode':data})

    # 로그인한 모든 사용자 바코드 값
    user_id_ref = db.reference("CG24/ID1/")
    total_user_dic = user_id_ref.get()
    users=total_user_dic.values()

    x=True
    # 사용자가 존재하면 바코드 값 1로 변경 
    while True : 
        for user_id in users:
            print("all user id:",user_id)
            
            # 현재 로그인한 사용자
            current_barcode_ref = db.reference("CG24/IDs/present_barcode")
            current_barcode_user = current_barcode_ref.get()
            print("current user: ", current_barcode_user)
        
            if (current_barcode_user==user_id):
                dir_barcode_exist=db.reference("barcode/")
                dir_barcode_exist.update({'barcode': "1"})
                x=False
                break

        if (x==False) :
            break

        else :
            print("sending barcode2")
    
            scode = str(input())
            
            print(scode)
            
            data = scode
            
            barcode = db.reference("CG24/IDs")
            barcode.update({'present_barcode':data})
            continue

         

    while True :
        #파이어베이스에서 1이라고 인식할 시
        ref1 = db.reference('barcode/barcode')
        ref = db.reference('barcode')

        #바코드 가지고 오는 코드
        row = ref1.get()
        print("%s" %(row))
        print(row)
        if (row == "5"):
            execute_camera()
            print("camera")
            
            print(row)
            ref.update({'barcode':"10"})
            break
            
        else : 
            continue

    
    
    sleep(2)
    
    
     
    

    
    
    
