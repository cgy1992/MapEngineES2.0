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
		 //��д�� Module���е��麯��  step 3����������ʵ��ֻ�ǻ��ظ��Ľ����������
		//������� ��ʱ�Ļ��ƣ������������ķѹ���ʱ�䣬��ʾ����ʧ��
		virtual pp::Instance* CreateInstance(PP_Instance instance) 
		{
			 pp::Instance* temp = new HelloTutorialInstance(instance); //����constructor�лᣬnew DrawEngine()
			 return temp;
		}
		/// Called by the browser when the module is first loaded and ready to run.
		/// This is called once per module, not once per instance of the module on
		/// the page.
		virtual bool Init()  //common.onload�������ʱ����һ��step2
		{
			return glInitializePPAPI(get_browser_interface()) == GL_TRUE;  //�ú�����ʼ�� ES������
		}
	};

	Module* CreateModule()   // ����һ��ģ��   step1
	{
		return new HelloTutorialModule();
	}
}  // namespace pp