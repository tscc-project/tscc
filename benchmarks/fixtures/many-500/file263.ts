interface Item263 { id: number; name: string; }
type Key263 = string | number;
export function make263(id: number, name: string): Item263 {
  const value: Item263 = {id, name};
  return value as Item263;
}
export const item263: Item263 = make263(263, "item-263");
