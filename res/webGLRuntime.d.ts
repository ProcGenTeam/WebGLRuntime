declare class Application {
  init(gl: WebGL2RenderingContext): void;
}

interface ApplicationConstructor {
  new (): Application;
}

interface ImGUIContext {
  beginWindow(title: string, callback: () => void): void;
  button(label: string): boolean;
  text(str: string): void;
  inputText(label: string, last: string): string;
  inputNumber(label: string, last: number): number;
  inputSlider(label: string, min: number, max: number, last: number): number;
  checkbox(label: string, last: boolean): boolean;
}

declare class Core {
  run(app: ApplicationConstructor): void;

  viewportWidth: number;
  viewportHeight: number;
}

declare const core: Core;
