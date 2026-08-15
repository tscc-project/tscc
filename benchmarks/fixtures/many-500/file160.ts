interface Item160 { id: number; name: string; }
type Key160 = string | number;
export function make160(id: number, name: string): Item160 {
  const value: Item160 = {id, name};
  return value as Item160;
}
export const item160: Item160 = make160(160, "item-160");
