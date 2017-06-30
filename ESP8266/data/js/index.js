var gaugeCor = document.getElementById('gaugeDACORRENTE');
var gaugePot = document.getElementById('gaugeDAPOTENCIA');
var porcentagemC = document.getElementById('valorCorrente');
var porcentagemP = document.getElementById('valorPotencia');
var grau;
var debug = false;

 var wsUri = "ws://" + extractHostname(location.href) + ":81";
//var output;

  function init()
  {
    //output = document.getElementById("output");
    testWebSocket();
  }

  function testWebSocket()
  {
    websocket = new WebSocket(wsUri);
    websocket.onopen = function(evt) { onOpen(evt) };
    websocket.onclose = function(evt) { onClose(evt) };
    websocket.onmessage = function(evt) { onMessage(evt) };
    websocket.onerror = function(evt) { onError(evt) };
  }

  function onOpen(evt)
  {
    writeToScreen("CONNECTED");
    doSend("CONECTADO");
  }

  function onClose(evt)
  {
    writeToScreen("DESCONECTADO");
  }

  function onMessage(evt)
  {
    writeToScreen('RECEBIDO: ' + evt.data);
    //websocket.close();
    var dados = JSON.parse(evt.data);
      grau_cor = dados.corrente / 20 * 180;
      grau_pot = dados.potencia / 4400 * 180;
      valorCorrente.innerHTML = dados.corrente+' A';
      valorPotencia.innerHTML = dados.potencia+' W';
      virar(grau_cor,grau_pot);
  }

  function onError(evt)
  {
    writeToScreen('ERRO: ' + evt.data);
  }
  
    function doSend(message)
  {
    writeToScreen("ENVIADO: " + message);
    websocket.send(message);
  }
  
    function writeToScreen(message)
  {
   if (debug) 
    {    
    console.log(message);
    }
  }


//Substituir por um botão "Conectar"
window.addEventListener("load", init, false);


//Ajusta os indicadores de acordo com o valor
function virar(grau_cor,grau_pot) {
var gaugeCor = document.getElementById('gaugeDACORRENTE');
var gaugePot = document.getElementById('gaugeDAPOTENCIA');
  
  gaugeCor.style.transform = 'rotate('+grau_cor+'deg)';
  gaugePot.style.transform = 'rotate('+grau_pot+'deg)';
}

//Extrai o hostname da página pra usar nos Websockets
//Retirado de https://stackoverflow.com/questions/8498592/extract-hostname-name-from-string
function extractHostname(url) {
    var hostname;
    //find & remove protocol (http, ftp, etc.) and get hostname

    if (url.indexOf("://") > -1) {
        hostname = url.split('/')[2];
    }
    else {
        hostname = url.split('/')[0];
    }

    //find & remove port number
    hostname = hostname.split(':')[0];
    //find & remove "?"
    hostname = hostname.split('?')[0];

    return hostname;
}
