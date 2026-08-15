interface Item372 { id: number; name: string; }
type Key372 = string | number;
export function make372(id: number, name: string): Item372 {
  const value: Item372 = {id, name};
  return value as Item372;
}
export const item372: Item372 = make372(372, "item-372");
