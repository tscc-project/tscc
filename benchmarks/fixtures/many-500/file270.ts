interface Item270 { id: number; name: string; }
type Key270 = string | number;
export function make270(id: number, name: string): Item270 {
  const value: Item270 = {id, name};
  return value as Item270;
}
export const item270: Item270 = make270(270, "item-270");
