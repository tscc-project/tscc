interface Item380 { id: number; name: string; }
type Key380 = string | number;
export function make380(id: number, name: string): Item380 {
  const value: Item380 = {id, name};
  return value as Item380;
}
export const item380: Item380 = make380(380, "item-380");
