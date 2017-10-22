console.log('yay');
var express = require('express'),
      path = require('path'),
      nconf  = require('nconf'),
      fs = require('fs'),
      bodyParser = require('body-parser'),
      exec = require('child_process').execFile,
      async = require('async');

//Fetch config data
nconf.file('./configs/bs-config.json');


// Init App and configurating
const app = express();
app.use(bodyParser({limit: '50mb'}));
app.use(bodyParser.urlencoded({ extended: false }));
app.use(bodyParser.json());

app.post('/img', function (req, res) {

  // res.send(req.respons);
  console.log(req.body.img);

  async.waterfall([
  // A list of functions
    function(callback){
        // Function no. 1 in sequence
        setTimeout(function(){console.log("run_hi_oid_AI")}, 1000);
        callback(null);
    },
    function(callback){
        // Function no. 2 in sequence
        setTimeout(function(){console.log("startReadBin")}, 2000);
        callback(null);
    }
  ],
  function(err, results){
    setTimeout(function(){console.log("iam the last one")}, 4000);
     // Optional final callback will get results for all prior functions
  });

  //read data from bin file
  fs.readFile('my-file.bin', 'utf8', function(err, data) {
    if (err) throw err;
    console.log(data);
  });

  //save data into txt file
  // fs.writeFile('helloworld.txt', req.query.img, function (err) {
  //   if (err)
  //       return console.log(err);
  //   console.log('Wrote Hello World in file helloworld.txt, just check it');
  // });

})

// Set Static Folder
console.log(nconf.get('public'));
app.use(express.static(path.join(__dirname, nconf.get('server').baseDir)));

// Set Port
app.set('port', (process.env.PORT || nconf.get('port')));

app.listen(app.get('port'), function(){
	console.log('Server started on port '+app.get('port'));
});

/* <-- sync funcs --> */
//sync read
function startReadBin(){
  var filePath = 'my-file.bin';
  var file = fs.readFileSync(filePath, 'utf8', function(err, data) {
    if (err) throw err;
    console.log(data);
  });
  fs.watchFile(filePath, function() {
      console.log('File Changed ...');
      file = fs.readFileSync(filePath, 'utf8');
      console.log('File content at : ' + new Date() + ' is \n' + file);
  });
}

function run_hi_oid_AI(){
  //Execute A.I.
  exec('src/ai_part/Hi-Oid.exe', function(err, data) {
        console.log(err)
        console.log(data.toString());
    });

}



//temp
