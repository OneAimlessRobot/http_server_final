

document.getElementById("upload").onclick = async () => {
    const file = document.getElementById("file").files[0];

    if (!file){
        return;
    }
	const file_target = encodeURIComponent(file.name)
	var response = null;
    try {
		
		alert("Clear this box 2 start uploadin!!!\nWait for the ding <3!");
		
		 response = await fetch(
			file_target,
			{
				method: "POST",
				headers: {
					"Target": "",
					"Content-Type": "application/octet-stream",
					"Specific": "upload"
					
					/*
					
						TODO: "Specific" tem muito potencial
						Posso tambem meter outras cenas!
						
					*/
				},
				body: file
			}
		);
		
		console.log(response.status);
    
		if(!response.ok){
			throw new Error(`HTTP ${response.status}`);
			
		}
		alert("DIIIIING~\nUpload complete! Yippieee!!!");
	}
	catch(err){
			alert("Upload failed! FAK-!\n");
			console.error(err);
	}
	if(response!=null){
		alert(`HTTP ${response.status}`);
	}
};
