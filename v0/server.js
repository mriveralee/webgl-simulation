var express = require('express');
var app = express();

app.use(express.static('public'));
app.use(express.static('examples'));
app.use(express.static('util'));
app.use(express.static('editor'));

app.use(express.static('stretch'));


app.get('/', function (req, res) {
  res.sendFile(path.join(__dirname + '/editor/index.html'));
});

app.listen(3000, function () {
  console.log('Example app listening on port 3000!');
});
