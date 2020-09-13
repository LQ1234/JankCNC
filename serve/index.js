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

    initSerial((name,handler)=>{
        events.addEventListener("dictionary", (e) => {
            let indx=e.data.indexOf(":");
            let key=e.data.slice(0,indx);
            let val=e.data.slice(indx+2);
            if(key==name)handler(val);
        });
    });
}
