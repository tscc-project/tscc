interface Item190 { id: number; name: string; }
type Key190 = string | number;
export function make190(id: number, name: string): Item190 {
  const value: Item190 = {id, name};
  return value as Item190;
}
export const item190: Item190 = make190(190, "item-190");
