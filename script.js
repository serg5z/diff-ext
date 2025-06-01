const themeToggle = document.getElementById('theme-toggle');

function setThemeIcon(isDark) {
  themeToggle.textContent = isDark ? "🌜" : "🌞";
}

// Toggle theme and update icon/persistence
themeToggle.addEventListener('click', () => {
  const isNowDark = !document.body.classList.contains('dark');
  document.body.classList.toggle('dark', isNowDark);
  setThemeIcon(isNowDark);
  localStorage.setItem('theme', isNowDark ? 'dark' : 'light');
});

// Apply saved or system theme on load
let theme = localStorage.getItem('theme');
if (!theme) {
  theme = window.matchMedia('(prefers-color-scheme: dark)').matches ? 'dark' : 'light';
}
const isDark = theme === 'dark';
document.body.classList.toggle('dark', isDark);
setThemeIcon(isDark);

// Active Nav
const navLinks = document.querySelectorAll('nav ul li a');
navLinks.forEach(link => {
  if(link.href === window.location.href) link.classList.add('active');
  else link.classList.remove('active');
});
/*
window.onload = function() {
  // Theme
  let theme = localStorage.getItem('theme');
  if (!theme) {
    theme = window.matchMedia('(prefers-color-scheme: dark)').matches ? 'dark' : 'light';
  }
  const isDark = theme === 'dark';
  setTheme(theme);
  setThemeIcon(isDark);

  // Active Nav
  const navLinks = document.querySelectorAll('nav ul li a');
  navLinks.forEach(link => {
    if(link.href === window.location.href) link.classList.add('active');
    else link.classList.remove('active');
  });
};
*/