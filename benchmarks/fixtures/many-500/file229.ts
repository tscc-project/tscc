interface Item229 { id: number; name: string; }
type Key229 = string | number;
export function make229(id: number, name: string): Item229 {
  const value: Item229 = {id, name};
  return value as Item229;
}
export const item229: Item229 = make229(229, "item-229");
