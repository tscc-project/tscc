interface Item212 { id: number; name: string; }
type Key212 = string | number;
export function make212(id: number, name: string): Item212 {
  const value: Item212 = {id, name};
  return value as Item212;
}
export const item212: Item212 = make212(212, "item-212");
