interface Item266 { id: number; name: string; }
type Key266 = string | number;
export function make266(id: number, name: string): Item266 {
  const value: Item266 = {id, name};
  return value as Item266;
}
export const item266: Item266 = make266(266, "item-266");
