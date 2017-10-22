//chrome.tabs.executeScript(tab.id, {file:"annyeng.js"});
//chrome.extension.getBackgroundPage().console.log("hi");
// chrome.tabs.query({active:true, currentWindow:true},function(tabs){
        //  chrome.tabs.executeScript(tabs[0].id, {file:"annyeng.js"});
// });

// chrome.tabs.executeScript(tabs[0].id, {file:"annyeng.js"});
chrome.runtime.sendMessage("cbcfmgddodbdldgcnkagddbijabnnima", {msg: "hello"}, function(response) {
  console.log(response.farewell);
});


chrome.extension.onMessage.addListener(function(request, sender, sendResponse) {
    if (request.type === "start") {
        setupAnnyang();
        start();
    }
    else if(request.type === "extmsg"){
      console.log("external message received"+" "+request.msg);
    }
    sendResponse({});
});

function send(request) {
    chrome.extension.sendMessage(request, function(response) {});
}

var BORDER_THICKNESS = 4;
var elminstector, outline;
var element, dimensions = {};

function mousemove(e) {
    if (element !== e.target) {
        element = e.target;
        console.log(element);
        dimensions.top = -window.scrollY;
        dimensions.left = -window.scrollX;
        var elem = e.target;
        while (elem !== document.body && elem !== null) {
            dimensions.top += elem.offsetTop;
            dimensions.left += elem.offsetLeft;
            elem = elem.offsetParent;
        }
        dimensions.width = element.offsetWidth;
        dimensions.height = element.offsetHeight

    }
}

function toggleCommand(hotword) {
    //end();
    dimensions.devicePixelRatio = window.devicePixelRatio;
    switch(hotword){
      case 'like':
      break;
      case 'dislike':
        setTimeout(function() { element.style.display="none"; }, 200);
        setTimeout(function() { send({ type: "GetImage", dimensions: dimensions }); }, 100);
        console.log('dislike request sent');
      break;
    }
    // document.body.removeChild(element);
}

function start() {
    if (!elminstector) {
        elminstector = document.createElement("div");
        elminstector.id = "el_capture_elminstector";
        elminstector.style.position = "absolute";
        elminstector.style.top = "0px";
        elminstector.style.left = "0px";
        elminstector.style.width = "100%";
        elminstector.style.height = "100%";
        elminstector.style.pointerEvents = "none";

    }
    if (!elminstector.parentNode) {
        document.body.appendChild(elminstector);
        document.body.addEventListener("mousemove", mousemove, false);

        //Stop whole thing by pressing esc!
        document.body.addEventListener("keyup", function(event){
            if (event.keyCode == 27) end();
        }, false);
    }
}
function setupAnnyang(){
  var commands = {
    'do not like it':
    function() {
        //start();
        toggleCommand('dislike');
        console.log('test!');
     }
  };
  // Add our commands to annyang
  annyang.addCommands(commands);

  // Start listening.
  annyang.start({ autoRestart: true, continuous: true });
}
function end() {
    document.body.removeChild(elminstector);
    document.body.removeEventListener("mousemove", mousemove, false);
}


send({ type: "enable" });
