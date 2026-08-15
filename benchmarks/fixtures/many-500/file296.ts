interface Item296 { id: number; name: string; }
type Key296 = string | number;
export function make296(id: number, name: string): Item296 {
  const value: Item296 = {id, name};
  return value as Item296;
}
export const item296: Item296 = make296(296, "item-296");
