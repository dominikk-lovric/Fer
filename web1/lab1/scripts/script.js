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
let id = 0;
function setStoreForId(id2) {
  id = id2;
  banimg = document.getElementById("bi2");
  let str = "assets/images/";
  str = str + data.categories[id2].path + "/" + data.categories[id2].image;
  banimg.setAttribute("src", str);
  banimg.setAttribute("alt", data.categories[id2].name);
  ht.innerText = data.categories[id].name;
  for (let i = 0; i < data.categories.length; i++) {
    let mar = (45 * 0.2) / data.categories.length;
    let fon = ((45 * 0.5) / (data.categories.length * 2 + 1)) * 2;
    if (i == id) {
      sid = document.getElementById("categories");
      if (window.innerWidth > 1024) {
        let str = (3 / 2) * fon + "vh";
        btns[i].firstChild.style.fontSize = str;
        str = mar + "vh 1vw";
        btns[i].style.margin = str;
      } else {
        let str = fon + "vh";
        btns[i].firstChild.style.fontSize = str;
        str = mar * (1 / 2) + "vh 1vw";
        btns[i].style.margin = str;
      }
      btns[i].firstChild.style.fontWeight = "bold";
    } else {
      btns[i].firstChild.style.fontWeight = "normal";
      if (window.innerWidth > 1024) {
        let str = fon + "vh";
        btns[i].firstChild.style.fontSize = str;
        str = mar + "vh 1vw";
        btns[i].style.margin = str;
      } else {
        let str = fon * (2 / 3) + "vh";
        btns[i].firstChild.style.fontSize = str;
        str = mar * (1 / 2) + "vh 1vw";
        btns[i].style.margin = str;
      }
    }
  }
  updateCts();
}
function updateCts() {
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
  let proiz = document.getElementById("pw");
  proiz.innerHTML = "";
  for (let i = 0; i < data.categories[id].products.length; i++) {
    proiz.appendChild(makeProduct(id, i));
  }
}

let btns = [];
for (let i = 0; i < data.categories.length; i++) {
  btns.push(makeIdButton(data.categories[i].name, i));
  let cats = document.getElementById("categories");
  cats.appendChild(btns[i]);
}

let ht = document.getElementById("header_text");

let cti = document.getElementById("cti");
let circpi = document.getElementById("circpi");
circpi.onclick = () => {
  document.location.href = "cart.html";
  sessionStorage.setItem("kolicine", JSON.stringify(kolicine));
};

circpi.style.opacity = 0;
setStoreForId(0);

function makeIdButton(S, a) {
  let div = document.createElement("div");
  div.setAttribute("class", "category");
  let butt = document.createElement("button");
  butt.style.backgroundColor = "transparent";
  butt.style.borderColor = "transparent";
  butt.innerText = S;
  butt.addEventListener("click", () => setStoreForId(a));
  let mar = (45 * 0.2) / data.categories.length;
  let fon = ((45 * 0.5) / (data.categories.length * 2 + 1)) * 2;
  butt.onmouseenter = () => {
    if (window.innerWidth > 1024) {
      let str = fon * (3 / 2) + "vh";
      butt.style.fontSize = str;
    } else {
      let str = fon + "vh";
      butt.style.fontSize = str;
    }
  };
  butt.onmouseleave = () => {
    if (a == id) return;
    if (window.innerWidth > 1024) {
      let str = fon + "vh";
      butt.style.fontSize = str;
    } else {
      let str = fon * (2 / 3) + "vh";
      butt.style.fontSize = str;
    }
  };

  div.appendChild(butt);
  return div;
}

function makeProduct(catId, proId) {
  div = document.createElement("div");
  div.setAttribute("class", "product");
  img = document.createElement("img");
  img.setAttribute("width", "80%");
  img.setAttribute("height", "90%");
  img.setAttribute("class", "product-img");
  let str = "assets/images/";
  str =
    str +
    data.categories[catId].path +
    "/" +
    data.categories[catId].products[proId].image;
  img.setAttribute("src", str);
  img.setAttribute("alt", data.categories[catId].products[proId].name);
  p = document.createElement("p");
  p.innerText = data.categories[catId].products[proId].name;
  circ = document.createElement("div");
  circ.setAttribute("class", "circle");
  circimg = document.createElement("img");
  circimg.setAttribute("width", "15%");
  circimg.setAttribute("height", "15%");
  circimg.setAttribute("src", "assets/images/circl.png");
  numb = document.createElement("div");
  numb.setAttribute("class", "circ-text");
  numb.innerText = kolicine[catId][proId];
  let bask = document.createElement("img");
  bask.setAttribute("class", "basket-product");
  bask.setAttribute("src", "assets/images/basket-outline.png");
  bask.setAttribute("position", "relative");
  bask.setAttribute("top", "-70%");
  bask.setAttribute("width", "30%");
  bask.setAttribute("height", "30%");
  bask.onclick = () => {
    kolicine[catId][proId]++;
    sessionStorage.setItem("kolicine", JSON.stringify(kolicine));
    updateCts();
  };
  bask.onmouseenter = () => {
    bask.style.opacity = 1;
  };
  bask.onmouseleave = () => {
    bask.style.opacity = 0;
  };
  img.onmouseenter = () => {
    bask.style.opacity = 1;
  };
  img.onmouseleave = () => {
    bask.style.opacity = 0;
  };

  circ.onmouseenter = () => {
    bask.style.opacity = 1;
  };
  circ.onmouseleave = () => {
    bask.style.opacity = 0;
  };
  circ.appendChild(circimg);
  circ.appendChild(numb);
  if (kolicine[catId][proId] <= 0) {
    circ.style.opacity = 0;
  }
  div.appendChild(img);
  div.appendChild(p);
  div.appendChild(circ);
  div.appendChild(bask);
  return div;
}

window.addEventListener("resize", () => setStoreForId(id));
