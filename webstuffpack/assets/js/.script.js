document.addEventListener("DOMContentLoaded", function() {

    let deferredPrompt;

    window.addEventListener("beforeinstallprompt", (event) => {
        event.preventDefault();  // Prevent the default browser install banner
        deferredPrompt = event;  // Store the event for later use

        // Show install button
        document.getElementById("install-btn").style.display = "block";

        document.getElementById("install-btn").addEventListener("click", () => {
            if (deferredPrompt) {
                deferredPrompt.prompt(); // Show the install prompt

                deferredPrompt.userChoice.then((choiceResult) => {
                    deferredPrompt = null; // Reset the prompt event
                });
            }
        });
    });

    if ("serviceWorker" in navigator) {
        navigator.serviceWorker.register("/js/.service-worker.js")
        .then((reg) => {
            console.log("Service Worker Registered!");
        })
        .catch((error) => {
            console.error("Service Worker Registration Failed:", error);
        });
    }
});
