interface Item319 { id: number; name: string; }
type Key319 = string | number;
export function make319(id: number, name: string): Item319 {
  const value: Item319 = {id, name};
  return value as Item319;
}
export const item319: Item319 = make319(319, "item-319");
