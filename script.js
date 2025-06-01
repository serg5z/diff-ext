// script.js
function setTheme(theme) {
  document.documentElement.setAttribute('data-theme', theme);
  localStorage.setItem('theme', theme);
}
function toggleTheme() {
  setTheme(document.documentElement.getAttribute('data-theme') === 'dark' ? 'light' : 'dark');
}
window.onload = function() {
  // Theme
  const saved = localStorage.getItem('theme');
  setTheme(saved ? saved : (window.matchMedia('(prefers-color-scheme: dark)').matches ? 'dark' : 'light'));

  // Active Nav
  const navLinks = document.querySelectorAll('nav ul li a');
  navLinks.forEach(link => {
    if(link.href === window.location.href) link.classList.add('active');
    else link.classList.remove('active');
  });
};
