#include "hello_world_test.h"
#include <windows.h>
namespace pp
{
	/// The Module class.  The browser calls the CreateInstance() method to create
	/// an instance of your NaCl module on the web page.  The browser creates a new
	/// instance for each <embed> tag with type="application/x-nacl".
	class HelloTutorialModule : public pp::Module 
	{
	public:
		HelloTutorialModule() : pp::Module() {}
		virtual ~HelloTutorialModule() 
		{
			glTerminatePPAPI();
		}
		 //重写了 Module类中的虚函数  step 3：创建两个实例只是会重复的进入这个方法
		//浏览器有 超时的机制，如果这个函数耗费过程时间，显示加载失败
		virtual pp::Instance* CreateInstance(PP_Instance instance) 
		{
			 pp::Instance* temp = new HelloTutorialInstance(instance); //它的constructor中会，new DrawEngine()
			 return temp;
		}
		/// Called by the browser when the module is first loaded and ready to run.
		/// This is called once per module, not once per instance of the module on
		/// the page.
		virtual bool Init()  //common.onload加载组件时调用一次step2
		{
			return glInitializePPAPI(get_browser_interface()) == GL_TRUE;  //该函数初始化 ES函数库
		}
	};

	Module* CreateModule()   // 创建一个模块   step1
	{
		return new HelloTutorialModule();
	}
}  // namespace pp