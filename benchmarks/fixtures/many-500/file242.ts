interface Item242 { id: number; name: string; }
type Key242 = string | number;
export function make242(id: number, name: string): Item242 {
  const value: Item242 = {id, name};
  return value as Item242;
}
export const item242: Item242 = make242(242, "item-242");
