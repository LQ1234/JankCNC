//Modified https://observablehq.com/@d3/zoomable-scatterplot
function createGraph(container){
    let height = 300;
    let width = 500;
    let numTicksX = 12;
    let heightWidthRatio=height/width;

    let updateGrid = (grid, xScale, yScale) => grid
        .attr("stroke", "black")
        .attr("stroke-opacity", 0.1)
        .call(grid => grid
            .selectAll(".x-grid-line")
            .data(xScale.ticks(numTicksX))
            .join(
                enter => enter.append("line").attr("class","x-grid-line").attr("y2",height),
                update => update,//??
                exit => exit.remove()
            )
                .attr("x1", d => 0.5 + xScale(d))
                .attr("x2", d => 0.5 + xScale(d)))
        .call(grid => grid
            .selectAll(".y-grid-line")
            .data(yScale.ticks(numTicksX*heightWidthRatio))
            .join(
                enter => enter.append("line").attr("class","y-grid-line").attr("x2",width),
                update => update,//??
                exit => exit.remove()
            )
                .attr("y1", d => 0.5 + yScale(d))
                .attr("y2", d => 0.5 + yScale(d)));

    let updateXAxis = (axis, xScale) => axis
        .attr("transform", `translate(0,${height-1})`)
        .call(d3.axisTop(xScale).ticks(numTicksX));

    let updateYAxis = (axis, yScale) => axis
        .attr("transform", `translate(0,-1)`)
        .call(d3.axisRight(yScale).ticks(numTicksX*heightWidthRatio));

    let xScaleToScreen = d3.scaleLinear()
        .domain([-4.5, 4.5])
        .range([0, width]);

    let yScaleToScreen = d3.scaleLinear()
        .domain([-4.5 * heightWidthRatio, 4.5 * heightWidthRatio])
        .range([height, 0]);

    let zoom = d3.zoom()
        .scaleExtent([0.005, 32])
        //.translateExtent([[0,10000],[1000000,0]].map(a=>[xScaleToScreen(a[0]),yScaleToScreen(a[1])]))
        .on("zoom", update);

    let svg = d3.select(container)
        .append("svg")
        .attr("class", "graph")
        .attr("viewBox", [0, 0, width, height]);

    let grid = svg.append("g");

    let datas={}

    let lines = svg.append("g");


    function fitData(lastms=10000,padding=.2){
        let minX=Infinity;
        let minY=Infinity;
        let maxX=-Infinity;
        let maxY=-Infinity;
        let cutoff=new Date().getTime()-lastms;

        Object.entries(datas).forEach(([name,data])=>{
            let tminX=Infinity;
            let tminY=Infinity;
            let tmaxX=-Infinity;
            let tmaxY=-Infinity;
            for(let i=data.length-1;i<0||data[i].t>cutoff;i--){
                if(i<0){
                    let w=tmaxX-tminX;

                    w/=(data[data.length-1].t-data[0].t)/lastms;
                    if(!w)w=0;
                    tminX=tmaxX-w;
                    break;
                }
                tminX=Math.min(data[i].x,tminX);
                tminY=Math.min(data[i].y,tminY);
                tmaxX=Math.max(data[i].x,tmaxX);
                tmaxY=Math.max(data[i].y,tmaxY);
            }
            minX=Math.min(tminX,minX);
            minY=Math.min(tminY,minY);
            maxX=Math.max(tmaxX,maxX);
            maxY=Math.max(tmaxY,maxY);
        });
        console.log(minX,minY,maxX,maxY)
        let w=maxX-minX;
        let h=maxY-minY;
        minX-=w*padding;
        minY-=h*padding;
        maxX+=w*padding;
        maxY+=h*padding;


        xScaleToScreen = d3.scaleLinear()
            .domain([minX, maxX])
            .range([0, width]);

        yScaleToScreen = d3.scaleLinear()
            .domain([minY,maxY])
            .range([height, 0]);
        svg.call(zoom).call(zoom.transform, d3.zoomIdentity);
    }

    let xAxis = svg.append("g");
    let yAxis = svg.append("g");

    svg.call(zoom).call(zoom.transform, d3.zoomIdentity);

    let legend = svg.append("g")
        .attr("transform", "scale(.7) translate(40,10)");

    let legendOrdinal = d3.scaleOrdinal(d3.schemeCategory10)
    let updateLegend = d3.legendColor()
        .scale(legendOrdinal);


    function update() {
        let transform = d3.event.transform;
        let xScale = transform.rescaleX(xScaleToScreen).interpolate(d3.interpolateRound);
        let yScale = transform.rescaleY(yScaleToScreen).interpolate(d3.interpolateRound);
        lines.selectAll("path").each(function(){
            d3.select(this)
                .attr("transform", transform)
                .attr("stroke-width", 5 / transform.k)
        });

        xAxis.call(updateXAxis, xScale);
        yAxis.call(updateYAxis, yScale);

        grid.call(updateGrid, xScale, yScale);
    }
    let autoscroll=false;
    let autoscrollLastX=null;
    let autoscrollChange=0;
    function addData(x,ys){
        let now=new Date().getTime();
        Object.entries(ys).forEach(([name,y])=>{
            //console.log("??",name);
            if(name in datas){
                datas[name].push({x:x,y:y,t:now});
            }else{
                datas[name]=[{x:x,y:y,t:now}];

            }
        })

        legendOrdinal.domain(Object.keys(datas));
        legend.call(updateLegend);
        //Object.entries(datas).map(a=>a[1])

        lines.selectAll("path").data(Object.entries(datas))
            .join(
                enter =>{

                    return(enter
                        .append("path")
                        .attr("fill", "none")
                        .attr("stroke", d=>legendOrdinal(d[0])));
                },
                update => update,
            )
            .each(function(d,i){
                d3.select(this)
                    .datum(d[1])
                    .attr("d", d3.line()
                        .x(d => xScaleToScreen(d.x))
                        .y(d => yScaleToScreen(d.y)))
            })

        if(autoscroll&&autoscrollLastX!=null){
            let diff=xScaleToScreen(x)-xScaleToScreen(autoscrollLastX);
            autoscrollChange+=-diff;
        }

        autoscrollLastX=x;

    }
    function scrollAutoscroll(){
        let change=autoscrollChange*.1;
        autoscrollChange-=change;
        svg.call(zoom.translateBy,change,0)
    }
    function clearData(){
        data=[];
    }
    setInterval(scrollAutoscroll,30);

    let fitButton = document.createElement("button");
    fitButton.textContent = "Fit";
    {
        fitButton.addEventListener("click", ()=>{
            fitData()
        });
    }
    let autoScrollCheckboxContainer = document.createElement("label");
    let autoScrollCheckbox = document.createElement("input");
    autoScrollCheckbox.type = "checkbox";
    autoScrollCheckbox.addEventListener("change", ()=>{autoscroll=autoScrollCheckbox.checked});

    autoScrollCheckboxContainer.appendChild(autoScrollCheckbox);
    autoScrollCheckboxContainer.appendChild(document.createTextNode("Auto Scroll"));

    let controlsDiv = document.createElement("div");
    controlsDiv.className = "controls";
    controlsDiv.appendChild(fitButton);
    controlsDiv.appendChild(autoScrollCheckboxContainer);

    container.appendChild(controlsDiv);

    return({add:addData,clear:clearData});
}
