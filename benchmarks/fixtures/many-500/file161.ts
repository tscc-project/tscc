interface Item161 { id: number; name: string; }
type Key161 = string | number;
export function make161(id: number, name: string): Item161 {
  const value: Item161 = {id, name};
  return value as Item161;
}
export const item161: Item161 = make161(161, "item-161");
