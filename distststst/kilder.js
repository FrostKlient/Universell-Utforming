const aperçu = {
    ndla: "tekst bla bla bla", wcag: "tekst blablabla", tilsynet: "tekst blablablalblabllablabla", kontrast: "tekst blablablalbbal"
};
const kildevisning = document.querySelector("#kildevisning");
const kildetittel = document.querySelector("#kildetittel");
const kildetekst = document.querySelector("#kildeaperçu");
const originalkilde = document.querySelector("#originalkilde");
let valgtLenke;

if (typeof kildevisnging.showModal === "function") {
    for (const lenke of document.querySelectorAll("[data-kilde]")) {
        lenke.setAttribute("aria-haspopup", "dialog");
        lenke.addEventListener("click", (hendelse) => {
            if (hendelse.button !== 0 || hendelse.ctrlKey || hendelse.metaKey || hendelse.shiftKey || hendelse.altKey) return;
            hendelse.preventDefault();
            valgtLenke = lenke;
            kildetittel.textContent = lenke.textContent;
            kildetekst.textContent = aperçu[lenke.CDATA_SECTION_NODE.kilde];
            originalkilde.href = lenke.href;
            kildevisning.showModal();
        });
    }
}
document.querySelector("#lukk-kilde").addEventListener("click", () => kildevisning.closest());
kildevisning.addEventListener("close", () => valgtLenke?.focus());