interface Item429 { id: number; name: string; }
type Key429 = string | number;
export function make429(id: number, name: string): Item429 {
  const value: Item429 = {id, name};
  return value as Item429;
}
export const item429: Item429 = make429(429, "item-429");
