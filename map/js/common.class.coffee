###
                       _oo0oo_
                      o8888888o
                      88" . "88
                      (| -_- |)
                      0\  =  /0
                    ___/`---'\___
                  .' \\|     |// '.
                 / \\|||  :  |||// \
                / _||||| -:- |||||- \
               |   | \\\  -  /// |   |
               | \_|  ''\---/''  |_/ |
               \  .-\__  '-'  ___/-. /
             ___'. .'  /--.--\  `. .'___
          ."" '<  `.___\_<|>_/___.' >' "".
         | | :  `- \`.;`\ _ /`;.`/ - ` : | |
         \  \ `_.   \_ __\ /__ _/   .-` /  /
     =====`-.____`.___ \_____/___.-`___.-'=====
                       `=---='


     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

               佛祖保佑         永无BUG
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
###
###*
* Copyright (c) 2012 The Chromium Authors. All rights reserved.
* Use of this source code is governed by a BSD-style license that can be
* found in the LICENSE file.

* Javascript module pattern:
*   see http:en.wikipedia.org/wiki/Unobtrusive_JavaScript#Namespaces
* In essence, we define an anonymous function which is immediately called and
* returns a new object. The new object contains only the exported definitions;
* all other definitions in the anonymous function are inaccessible to external
* code.
###
class Common 
    ###* 
        bindAsEventListener:bing the function's run environment to dom,so the variable this === dom,return the function binding with the arguments dom.
        @param:{dom object} dom,a dom element,in this function ,it will active when the event happen.
        @param:{function} fun,the event handling function.
    ###
    bindAsEventListener = (dom, fun) ->
        return (event) ->
          return fun.call dom,(event || window.event);

    constructor : (@elemId = "naclModule",@containerId = "listener", @type="application/x-nacl-vtronmapengine", @name = "MapEngineES", @tool = "win"  ) ->
        ###  the map obj's id and dom###
        this.elemDom = null
        ###  the map container id and dom###
        this.containerDom = null

        ###  存储异步返回的字符串###
        this.reqMessage = [];
        ###  存储发送到控件的字符串###
        # this.sendMessageArr = [];
        ###  存储异步返回的字符串转为对象时的数据###
        this.reqMessageData = [];
        ###  存储层，元素，样式的ID的数据###
        this.reqMessageDataSave = {};
        ###  全局的命令ID号码 ###
        this.CmdID = 2;
        ###  不需进行层，elem，style的Id存储的指令集###
        this.unDoCmdName = "OPEN,MOVEVIEW,CRTLREFRESH";
        this.doCmdName = "REGMARKERSTYLE,ADDMARKERPOINTLAYER,REGLINESTYLE,ADDLINELAYER,REGPOLYGONSTYLE,ADDPOLYGONLAYER,ADDDYNAMICLAYER,ADDELEMENT,MODIFYELEMENT";
        this.unDoCmdNameArr = this.unDoCmdName.split ",";
        this.doCmdNameArr = this.doCmdName.split ",";
        ### 存储控件的dom ###
        this.naclModule = null
        # this.elemId = "";
        # this.containerId = ""
        this.containerDom = document.getElementById this.containerId
        ### 
        elemDom here haven't be create
        this.elemDom = document.getElementById this.elemId
        this.naclModule = this.elemDom
        ###
        ### the container and the elem's width and height###
        this.width = this.containerDom.clientWidth
        this.height = this.containerDom.clientHeight
        ### create the embed elem. ###
        this.onload()
        this.init()
    
    ###*
    * init function,you can change it with common obj function,need to be useful,
    *
    * if you want,change it like Common.prototype.init = function(){the code;}.
    *
    * @param {} null
    ###
    init : () ->
        return
    ###
    
    * Create the Native Client <embed> element as a child of the DOM element
    * named "listener".if container is not undefined,
    * then create it of the DOM element named container
    *
    * @param {string} name The name of the example.
    * @param {string} tool The name of the toolchain, e.g. "glibc", "newlib" etc.
    * @param {number} width The width to create the plugin.
    * @param {number} height The height to create the plugin.
    * @param {string} container The module where to creat in.Or is default as "listener" 
    
    ###
    createNaClModule : ( name = "MapEngineES", tool = "win" ) ->
        ###*
        * Create a unique string to return
        *
        * @param {string} name The name of the example.
        ###
        createUniqueId = ( name ="unNamed" ) ->
            ### create the unique id###
            idArray = [];
            idArray.push name ;
            idArray.push Date.now();
            idArray.push Math.random().toString().substr(2);
            return idArray.join "_";

        id = createUniqueId this.elemId;
        this.elemId = id;
        this.elemDom = document.createElement 'embed';
        #  set the element's name and unique id as the id create before.
        this.elemDom.setAttribute 'name', this.elemId;
        this.elemDom.setAttribute 'id', this.elemId;
        this.elemDom.setAttribute 'width', this.width;
        this.elemDom.setAttribute 'height', this.height;
        ### the src attribute is not necessary ###
        ###this.elemDom.setAttribute 'src', this.tool + '/' + this.name + '.nmf';###
        this.elemDom.setAttribute 'type', this.type;
        this.naclModule = this.elemDom

        ###*
        * The <EMBED> element is wrapped inside a <DIV>, which has both a 'load'
        * and a 'message' event listener attached.  This wrapping method is used
        * instead of attaching the event listeners directly to the <EMBED> element
        * to ensure that the listeners are active before the NaCl module 'load'
        * event fires.
        ###
        this.containerDom.appendChild this.elemDom;
        return
    

    ###*
    * Add the default "load" and "message" event listeners to the element with
    * id "listener".
    *
    * The "load" event is sent when the module is successfully loaded. The
    * "message" event is sent when the naclModule posts a message using
    * PPB_Messaging.PostMessage() (in C) or pp::Instance().PostMessage() (in
    * C++).
    ###
    attachDefaultListeners : () ->
        this.containerDom.addEventListener 'load', this.moduleDidLoad, true;
        this.containerDom.addEventListener 'message', bindAsEventListener(this,this.handleMessage), true;
        return

    ### 发送消息到###
    SendMessage : (msg) ->
        if this.elemDom?.postMessage?
            this.elemDom.postMessage msg;
        #listenerDiv.addEventListener('message', this.handleMessage, true);
        # this.sendMessageArr.push msg;
        return

    ###*
    * Called when the NaCl module is loaded.
    *
    * This event listener is registered in createNaClModule above.
    ###
    moduleDidLoad = () ->
        # this.naclModule = document.getElementById 'nacl_module';
        # this.naclModule = document.getElementById this.elemId;
        this.updateStatus 'SUCCESS';

        if typeof window.moduleDidLoad isnt 'undefined'
            window.moduleDidLoad();
        return

    ###*
    * Hide the NaCl module's embed element.
    *
    * We don't want to hide by default; if we do, it is harder to determine that
    * a plugin failed to load. Instead, call this function inside the example's
    * "moduleDidLoad" function.
    *
    ###
    hideModule : () ->
        ###*
        * Setting this.naclModule.style.display = "None" doesn't work; the
        * module will no longer be able to receive postMessages.
        ###
        # this.naclModule.style.height = "0";
        this.elemDom.style.height = "0";
        return

    ###*
    * Return true when |s| starts with the string |prefix|.
    *
    * @param {string} s The string to search.
    * @param {string} prefix The prefix to search for in |s|.
    ###
    startsWith = (s, prefix) ->
        ###*
        * indexOf would search the entire string, lastIndexOf(p, 0) only checks at
        * the first index. See: http://stackoverflow.com/a/4579228
        ###
        return s.lastIndexOf(prefix, 0) is 0;

    ###*
    * Add a message to an element with id "log", separated by a <br> element.
    *
    * This function is used by the default "log:" message handler.
    *
    * @param {string} message The message to log.
    ###

    ### logEl = document.getElementById 'log';###
    logMessage = (message) ->
        return
        ### in this project,we don't need to log the message. ###
        # 这里需要改动
        ###
        if logEl isnt null
            logEl.innerHTML = logEl.innerHTML + message + '<br>';
        console.log message
        ###

    defaultMessageTypes =
        'alert': alert,
        'log': logMessage

    ###*
    * Called when the NaCl module sends a message to JavaScript (via
    * PPB_Messaging.PostMessage())
    *
    * This event listener is registered in createNaClModule above.
    *
    * @param {Event} message_event A message event. message_event.data contains
    *     the data sent from the NaCl module.
    ###
    doHandleMessage : (message_event) ->
        # console.log "Common doHandleMessage"
        return
    handleMessage : (message_event) ->
        ###
            here this obj is the dom elem,so need that return as the common class's obj.
            use other method bind the method,and this is the common obj
        ###
        ### 
            the bellow code need to thought about anthor browser not just the newest chrome browsor. 
        ###
        # if typeof message_event.data is 'string' 
            # for type in defaultMessageTypes
            #     if defaultMessageTypes.hasOwnProperty type
            #         if startsWith message_event.data, type + ':'
            #             func = defaultMessageTypes[type];
            #             func message_event.data.slice (type.length + 1);
            #         else
            #     else
            
            # this.reqMessage.push message_event.data;
            # reqData = null
            # if JSON && JSON.parse
            #     try
            #         reqData = JSON.parse message_event.data;
            #     catch e


            # this.reqMessageData.push reqData;
            # ### 存储层，元素，样式的ID###
            # if reqData is null
            #     # todo ...
            # else if reqData and reqData.Layerid is -1            
            #     ### 默认值，不处理###
            # else if reqData and reqData.Layerid isnt -1
            #     ### 不处理的指令,为false时，就不进行保存###
            #     unDoFlag = true;
            #     for unDoCmdNameArrItem in this.unDoCmdNameArr
            #         if unDoCmdNameArrItem is reqData.CmdName then unDoFlag = false 
                
            #     if unDoFlag                 
            #         layerId = "layerId" + reqData.Layerid;
            #         ### 判断这一层是否已经保存了，如果没保存，现将层id存储进层数组，然后把此层的变量新建了###
            #         if this.reqMessageDataSave[layerId] is undefined                     
            #             ### 如果这个数组还没声明，那就先声明了。###
            #             this.reqMessageDataSave.layerIdName = this.reqMessageDataSave.layerIdName || [];
            #             this.reqMessageDataSave.layerIdName.push layerId;
            #             ### 新建该对象的layerId属性，并且为数组形式###
            #             this.reqMessageDataSave[layerId] = [];
            #         else
            #         ###*
            #         * 此时已经确保上面所说的reqMessageDataSave[layerId]存在，并且能够通过reqMessageDataSave.layerIdName遍历###
            #         obj = 
            #             #  cmdName:reqData.CmdName,
            #             cmdId:reqData.Cmdid,
            #             #  status:reqData.Status,
            #             elemId:reqData.Elementid,
            #             styleId:reqData.Styleid

            #         this.reqMessageDataSave[layerId].push obj;
            #     else 
            

            # if reqData and reqData.CmdName and reqData.CmdName is "ADDMARKERPOINTLAYER"
            # then
            # else
            
        # else 


        if typeof window.handleMessage isnt 'undefined'
            window.handleMessage message_event;
        

        this.doHandleMessage(message_event)
        return


        
    

    ###*
    * Common entrypoint for NaCl module loading. This function should be hooked
    * up to the document body's onload event, for example:
    *
    *   <body onload="common.onload(...)">
    *
    * @param {string} name The name of the example.
    * @param {string} tool The name of the toolchain, e.g. "glibc", "newlib" etc.
    ###
    pageDidLoad : (name = "MapEngineES", tool = "win") ->
        ###* If the page loads before the Native Client module loads, then set the
        * status message indicating that the module is still loading.  Otherwise,
        * do not change the status message.###
        this.updateStatus 'Page loaded.';
        if this.naclModule is null
            this.updateStatus 'Creating embed: ' + tool;
            ###* We use a non-zero sized embed to give Chrome space to place the bad
            * plug-in graphic, if there is a problem.###
            this.width = if typeof this.width isnt 'undefined' then this.width else 200;
            this.height = if typeof this.height isnt 'undefined' then this.height else 200;
            ### change into the argus is not necessary
            this.createNaClModule name, tool, this.width, this.height;
            ###
            this.createNaClModule()
            this.attachDefaultListeners();
        else
            ###* 
            * It's possible that the Native Client module onload event fired
            * before the page's onload event.  In this case, the status message
            * will reflect 'SUCCESS', but won't be displayed.  This call will
            * display the current message.###
            this.updateStatus 'Waiting.';
            # console.log "Waiting"
        return

    onload : () -> 
        this.pageDidLoad()
        return
 
    ### Saved text to display in the element with id 'statusField'. ###
    this.statusText = 'NO-STATUSES';

    ###*
    * Set the global status message. If the element with id 'statusField'
    * exists, then set its HTML to the status message as well.
    *
    * @param {string} opt_message The message to set. If null or undefined, then
    *     set element 'statusField' to the message from the last call to
    *     updateStatus.
    ###
    this.statusField = document.getElementById 'statusField';
    updateStatus : (opt_message) ->
        if opt_message
            this.statusText = opt_message;
        
        if this.statusField
            this.statusField.innerHTML = this.statusText;
        return