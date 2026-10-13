

function generate_upload_with_target(target_name){
	
	
		const file = document.getElementById("file").files[0];

		if (!file){
			return;
		}
		
		const file_target = encodeURIComponent(file.name)
		alert("Clear this box 2 start uploadin!!!\nWait for the ding <3!");
			
		return fetch(
			file_target,
			{
				method: "PUT",
				headers: {
					"Target": target_name,
					"Content-Type": "application/octet-stream",
					"Specific": "upload"
					
					/*
					
						TODO: "Specific" tem muito potencial
						Posso tambem meter outras cenas!
						
					*/
				},
				body: file
			}
		)
    .then(response=>{
		console.log(response.status);
	
		if(!response.ok){
			alert(`HTTP ${response.status}`);
			throw new Error(`HTTP ${response.status}`);
			
		}
		
		if(response!=null){
			alert("DIIIIING~\nUpload complete! Yippieee!!!" + `Non null response! HTTP ${response.status}`);
		}
		else{
			
			alert("Upload failed! FAK-!\n");
			console.error(err);
		}
		
		})
		
		
}
