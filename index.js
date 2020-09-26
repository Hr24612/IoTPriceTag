const express = require('express');
const bodyparser = require('body-parser');
const fs = require('fs');

const app = express();
app.set('view engine', 'ejs');

app.use(bodyparser.json({ limit: '10mb', extended: true }));
app.use(bodyparser.urlencoded({ limit: '10mb', extended: true }));

var output;

app.get('/', (req, res) => {
	res.render('home');
});

app.post('/upload', (req, res) => {
	output = req.body.output;

	fs.writeFile('output.txt', output, function (err) {
		if (err) {
			console.log(err);
		}
	});
	chunked();
	res.redirect('/');
});

var chunksArray = [];

const chunked = () => {
	fs.readFile('output.txt', 'utf8', (err, data) => {
		const chunkSize = 5000;

		const n = data.length % chunkSize;

		chunksArray.push(data.slice(0, n));
		for (let start = n; start < data.length - n; start += chunkSize) {
			chunksArray.push(data.slice(start, start + chunkSize));
		}
		//console.log(chunksArray.length);
	});
};
var index = 0;
app.get('/getdata', (req, res) => {
	if (chunksArray.length > index) {
		console.log(index);
		res.send(chunksArray[index]);
	} else {
		res.send('Done!');
	}
	index++;
});

let port = process.env.PORT;
if (port == null || port == '') {
	port = 3000;
}

app.listen(port, () => {
	console.log('Server is listening');
});
