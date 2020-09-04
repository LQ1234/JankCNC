window.onload=()=>{

    let graph=createGraph(document.getElementById("leftrightgraph"));
    let graph2=createGraph(document.getElementById("updowngraph"));

    function test(p){
        let t=new Date().getTime();
        graph.add(t,{[p]:Math.random()});
        setTimeout(()=>test(p),Math.random()*1000);
    }
    Array(5).fill(0).map(a=>Math.random().toString(36).substring(7)).forEach(test)

    var events = new EventSource('eventstream');

    events.onmessage = function(event) {
        console.log(event.data);
    }
}
