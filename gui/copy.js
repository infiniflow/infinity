const fs = require('fs');
const path = require('path');

const nextStaticPath = path.join(__dirname, '.next', 'static');
const nextStandalonePath = path.join(
  __dirname,
  '.next',
  'standalone',
  'public',
  '_next',
  'static'
);

function copyFolderRecursiveSync(src, dest) {
  const exists = fs.existsSync(src);
  const stats = exists && fs.statSync(src);
  const isDirectory = exists && stats.isDirectory();
  if (isDirectory) {
    fs.mkdirSync(dest, { recursive: true });
    fs.readdirSync(src).forEach((childItemName) => {
      copyFolderRecursiveSync(
        path.join(src, childItemName),
        path.join(dest, childItemName)
      );
    });
  } else {
    fs.copyFileSync(src, dest);
  }
}

copyFolderRecursiveSync(nextStaticPath, nextStandalonePath);
