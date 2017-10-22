console.log('yay');
var express = require('express'),
      path = require('path'),
      nconf  = require('nconf');
var bodyParser = require('body-parser');

//Fetch config data
nconf.file('./configs/bs-config.json');


// Init App
const app = express();
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));

app.get('/img', function (req, res) {

  // res.send(req.respons);
  console.log(req.query.img);
})

// Set Static Folder
console.log( nconf.get('public'));
app.use(express.static(path.join(__dirname, nconf.get('server').baseDir)));

// Set Port
app.set('port', (process.env.PORT || nconf.get('port')));

app.listen(app.get('port'), function(){
	console.log('Server started on port '+app.get('port'));
});
