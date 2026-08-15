interface Item221 { id: number; name: string; }
type Key221 = string | number;
export function make221(id: number, name: string): Item221 {
  const value: Item221 = {id, name};
  return value as Item221;
}
export const item221: Item221 = make221(221, "item-221");
