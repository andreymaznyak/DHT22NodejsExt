

const Waterline = require('waterline'), 
	  app = {},
	  DHT = require('./build/Release/dht22.node');
// Instantiate a new instance of the ORM
var orm = new Waterline();


//////////////////////////////////////////////////////////////////
// WATERLINE CONFIG
//////////////////////////////////////////////////////////////////

// Require any waterline compatible adapters here
var diskAdapter = require('sails-postgresql');


// Build A Config Object
var config = {

  // Setup Adapters
  // Creates named adapters that have been required
  adapters: {
    'default': diskAdapter,
    'sails-postgresql': diskAdapter
  },

  // Build Connections Config
  // Setup connections using the named adapter configs
  connections: {
    myLocalPostgres: {
      adapter: 'sails-postgresql',
      host: 'localhost',
      database: 'Sensors',
      user:'orangepi',
      password: require('./password.json').value
    }
  },

  defaults: {
    migrate: 'alter'
  }

};

//////////////////////////////////////////////////////////////////
// WATERLINE MODELS
//////////////////////////////////////////////////////////////////

var humidity = Waterline.Collection.extend({

  identity: 'humidity',
  connection: 'myLocalPostgres',

  attributes: {
    value: 'integer'
  }
});
var temperature = Waterline.Collection.extend({

  identity: 'temperature',
  connection: 'myLocalPostgres',

  attributes: {
    value: 'float'
  }
});

orm.loadCollection(humidity);
orm.loadCollection(temperature);

// Start Waterline passing adapters in
orm.initialize(config, function(err, models) {
  if(err) throw err;

  let current_humidity = 0.0, current_temperature = 0.0; 
  app.models = models.collections;
  app.connections = models.connections;

  setInterval(() => {
  	let sensor_data = DHT.getDHT22Data();
  	if(typeof(sensor_data) != 'string'){
  		if(current_humidity != Math.floor(sensor_data.Humidity)){
  			current_humidity = Math.floor(sensor_data.Humidity);
  			app.models.humidity.create({value:current_humidity}).exec((err, res)=> console.log('add humidity to db') );
  			
	  	}
	  	if(current_temperature != sensor_data.Temperature){
	  		current_temperature = sensor_data.Temperature;
	  		app.models.temperature.create({value:current_temperature}).exec((err, res)=> console.log('add temperature to db') );
	  	}
  	}
  	
  	console.log(sensor_data);
  },5000);

  console.log("DHT22 service started");
});