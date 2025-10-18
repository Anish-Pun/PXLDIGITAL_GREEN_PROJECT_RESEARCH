let text;
try {
    text = msg.payload.toString('utf8'); // decode bytes
} catch (e) {
    text = JSON.stringify(msg.payload);  // fallback like Python repr()
}
msg.payload = text;
return msg;
