# Cloud libraries 

sources = [
	'CEFApp.h',
	'CEFApp.cpp',
	'CEFImp.h',
	'CEFURLRequestController.h',
	'CloudController.h',
	'CloudController.cpp',
	'CloudControllerFactory.h',
	'CloudControllerFactory.cpp',
	'CloudImp.h',
	'CloudMessage.h',
	'CloudMessage.cpp',
	'CloudModel.h',
	'CloudRequest.h',
	'CloudResponse.h',
	'WebRequest.h',
	'WebRequest.cpp',
	'WebRequestPostData.h',
	'WebRequestPostData.cpp',
	'WebRequestPostDataElement.h',
	'WebRequestPostDataElement.cpp'
]

cloud = shared_library('cloud', [sources])

subdir('environment')
subdir('http')
subdir('menu')
subdir('user')
subdir('webrtc')
subdir('websockets')