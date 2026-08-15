interface Item121 { id: number; name: string; }
type Key121 = string | number;
export function make121(id: number, name: string): Item121 {
  const value: Item121 = {id, name};
  return value as Item121;
}
export const item121: Item121 = make121(121, "item-121");
