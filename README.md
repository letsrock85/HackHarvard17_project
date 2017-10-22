# Hi-Oid

# Inspiration
One member of our team brought us a touching story about how a member of his family with a visual disability struggles to use internet and computers. Something that is so common in our lives but that we don't appreciate. Our goal for this Hackathon was to put ourselves in a blind person shoes and try to use the computer. That lead us to create a voice based extension that later we improved adding AI to learn from our browsing behavior and to expand also the possibilities of our experience.

# What it does
When the extension is activated. It waits till it receives a voice command. Execute it. The AI learn it and retrain a model based on our browsing behavior. Wash, Rinse and Repeat



# INSTRUCTION!
To run locally     
1. Install nodejs and npm. Then install gulp package globally    
2. Locate project folder in terminal      
3. Type "npm install" to install node packages   
4. Type "npm run nodeapp" to start the server and app

Then you will have to install Chrome Extension locally.
To do that, follow the instructions bellow:
1. Navigate to chrome://extensions
2. Expand the Developer dropdown menu and click “Load Unpacked Extension”
3. Navigate to the local folder containing the extension’s (src/ai_extension) code and click Ok
4. Assuming there are no errors, the extension should load into your browser 

Once the extension has been successfully loaded into your browser, navigate to facebook.com, click on the extension icon 
"A.I." in the top right app drawer and then click allow when the browser asks to use your microphone so it can listen to voice commands. 

Now while scrolling your feed you can simply say "dislike it" and then the Hi-Oid will hide it, it will then learn the kinds of photos that you don't like. After using it more and more the artificial intellence will improve your browsering experience by removing any unpleasant posts. For the purpose of this demo, make sure the cursor is on the post or picture and then repeat the voice command.




