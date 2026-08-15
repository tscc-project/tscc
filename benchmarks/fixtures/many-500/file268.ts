interface Item268 { id: number; name: string; }
type Key268 = string | number;
export function make268(id: number, name: string): Item268 {
  const value: Item268 = {id, name};
  return value as Item268;
}
export const item268: Item268 = make268(268, "item-268");
