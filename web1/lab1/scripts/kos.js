let kolicine = sessionStorage.getItem("kolicine");
if (kolicine) {
  kolicine = JSON.parse(kolicine);
  if (kolicine.length != data.categories.length) kolicine = setKolicine();
  for (let i = 0; i < data.categories.length; i++) {
    if (kolicine[i].length != data.categories[i].products.length)
      kolicine = setKolicine();
  }
} else {
  kolicine = setKolicine();

  sessionStorage.setItem("kolicine", JSON.stringify(kolicine));
}

function setKolicine() {
  let k = [];
  for (let i = 0; i < data.categories.length; i++) {
    let pr = [];
    for (let j = 0; j < data.categories[i].products.length; j++) {
      pr.push(0);
    }
    k.push(pr);
  }
  return k;
}

let itms = [];
let kos = document.getElementById("kos");
let pz = document.getElementById("proz");
let ko = document.getElementById("koli");
let cti = document.getElementById("cti");
let circpi = document.getElementById("circpi");

circpi.onclick = () => {
  window.location.href = "./index.html";
};

function setKol(a, cat, num) {
  div = document.createElement("div");
  div.style.display = "flex";
  div.setAttribute("flex-direction", "row");
  div.style.alignItems = "center";
  div.style.justifyContent = "center";

  t2 = document.createElement("p");
  t2.innerText = a;
  t2.style.fontSize = "5vh";
  t2.setAttribute("id", "id" + kos.childElementCount);
  bt1 = document.createElement("button");
  bt1.style.display = "flex";
  bt1.style.justifyContent = "center";
  bt1.style.alignItems = "center";
  bt1.style.fontSize = "5vh";
  bt1.style.height = "4vh";
  bt1.style.width = "4vh";
  bt1.style.borderRadius = "50%";
  bt1.style.textAlign = "center";
  bt1.innerText = "+";
  bt1.onclick = () => {
    kolicine[cat][num - 1]++;
    sessionStorage.setItem("kolicine", JSON.stringify(kolicine));
    setCart();
  };
  bt2 = document.createElement("button");
  bt2.style.display = "flex";
  bt2.style.justifyContent = "center";
  bt2.style.alignItems = "center";
  bt2.style.fontSize = "5vh";
  bt2.style.height = "4vh";
  bt2.style.width = "4vh";
  bt2.style.borderRadius = "50%";
  bt2.style.textAlign = "center";
  bt2.innerText = "-";
  bt2.onclick = () => {
    kolicine[cat][num - 1]--;
    sessionStorage.setItem("kolicine", JSON.stringify(kolicine));
    setCart();
  };
  div.appendChild(bt2);
  div.appendChild(t2);
  div.appendChild(bt1);
  num++;
  return div;
}

function setStr(S) {
  div = document.createElement("div");
  div.style.display = "flex";
  div.setAttribute("flex-direction", "row");
  div.style.alignItems = "center";
  div.style.justifyContent = "center";

  t1 = document.createElement("p");
  t1.innerText = S;
  t1.style.fontSize = "5vh";
  div.appendChild(t1);
  return div;
}

function setCart() {
  let nn = 0;
  for (let i = 0; i < data.categories.length; i++) {
    for (let j = 0; j < data.categories[i].products.length; j++) {
      nn += kolicine[i][j];
    }
  }
  if (nn > 0) {
    cti.innerText = nn;
    circpi.style.opacity = 1;
  }
  let kos = document.getElementById("kos");
  let pz = document.getElementById("proz");
  pz.innerHTML = "";
  pz.appendChild(setStr("Prozivodi"));
  for (let i = 0; i < data.categories.length; i++) {
    for (let j = 0; j < data.categories[i].products.length; j++) {
      if (kolicine[i][j] > 0)
        pz.appendChild(setStr(data.categories[i].products[j].name));
    }
  }
  let ko = document.getElementById("koli");
  ko.innerHTML = "";
  ko.appendChild(setStr("Količine"));
  ko.appendChild(setStr(""));
  for (let i = 0; i < data.categories.length; i++) {
    for (let j = 0; j < data.categories[i].products.length; j++) {
      if (kolicine[i][j] > 0) ko.appendChild(setKol(kolicine[i][j], i, j));
    }
  }
}

let gs = document.getElementById("goShop");
gs.onclick = () => {
  sessionStorage.setItem("kolicine", JSON.stringify(kolicine));
};
circpi.style.opacity = 0;
setCart();
