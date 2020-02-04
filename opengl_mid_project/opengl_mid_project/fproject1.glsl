uniform int i;

void main(void)
{
if(i==0){
    gl_FragColor = vec4(0.95686,0.86274,0.79607,1.0);
	}
	else if(i==1){
	gl_FragColor=vec4(0.31372,0.30588,0.31764,1.0);
	}
	else if(i==2){
	gl_FragColor=vec4(0.31372,0.30588,0.31764,1.0);
	}
	else if(i==5){
	gl_FragColor=vec4(0.92941,0.79215,0.67843,1.0);
	}
	else if(i==6){
	gl_FragColor=vec4(0.7,0.0,0.0,1.0);
	}
	else if(i==7){
	gl_FragColor=vec4(0.31372,0.30588,0.31764,1.0);
	}
	else{
	gl_FragColor=vec4(0.0,0.0,0.0,1.0);
	}
}
