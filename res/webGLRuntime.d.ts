declare class Application {
  init(gl: WebGL2RenderingContext): void;
}

interface ApplicationConstructor {
  new (): Application;
}

interface ImGUIContext {
  button(text: string): boolean;
}

declare class Core {
  run(app: ApplicationConstructor): void;

  viewportWidth: number;
  viewportHeight: number;
}

declare const core: Core;
