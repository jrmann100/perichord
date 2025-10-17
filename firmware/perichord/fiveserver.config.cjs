module.exports = {
  port: 8000,
  open: false,
  middleware: [
    (_, res, next) => {
      // apparently required for AudioWorklet to work
      res.setHeader("Cross-Origin-Opener-Policy", "same-origin");
      res.setHeader("Cross-Origin-Embedder-Policy", "require-corp");
      next();
    },
  ],
};
