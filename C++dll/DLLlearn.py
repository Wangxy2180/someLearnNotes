import ctypes
import os
import cv2
# CUR_PATH=os.path.dirname(__file__)
# dll_path = os.path.join(CUR_PATH,"Project1.dll")
print(__file__)
# dll_path = os.path.join(CUR_PATH,"01_C_sample.dll")
# print(dll_path)
# pDLL=ctypes.WinDLL(dll_path)
# lla=ctypes.cdll.LoadLibrary("03_C++_sample_OpenCV.dll")
# lla.welcome()
# pDLL=ctypes.WinDLL("02_C++_simple_sample.dll")
# print(pDLL)
print("-"*30)
# re1=pDLL.sum(1,2)
# pDLL.print_welcome_c()
# pDLL.print_custom_c("大家好".encode('utf8'))
# ctypes.CDLL.
# print(re1)
# ret=lla.ret_str()
# aaa=ctypes.string_at(ret).decode('utf8')
# print(aaa)
# aa=ctypes.string_at(lla.ret_str()).decode('utf8')
# print(aa)
lla=ctypes.cdll.LoadLibrary("03_C++_sample_OpenCV.dll")
lla.welcome()
img_gray=lla.img_to_gray()
print(img_gray)
cv2.imshow("123",img_gray)